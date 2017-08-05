#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

#include "apriltag.h"
#include "tag36h11.h"

#include "common/image_u8.h"
#include "common/zarray.h"

#include "emscripten.h"

typedef void (*detected)(
    int,            // id

    double, double, // points
    double, double,
    double, double,
    double, double,

    double, double, double, // 3x3 matrix
    double, double, double,
    double, double, double
);

int detect(detected cb, int w, int h, unsigned char *buf) {
    emscripten_log(0, "detecting in %dx%d image", w, h);

    const unsigned char *p = buf;
    unsigned char *gray = malloc(w * h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            double r = *p / 255.0; p++;
            double g = *p / 255.0; p++;
            double b = *p / 255.0; p++;
            p++; // ignore alpha

            // double alpha = 0.42;
            // double v = 0.5 + log(g) - alpha*log(b) - (1-alpha)*log(r);
            double v = r * .2126 + g * .7152 + b * .0722;

            int iv = v * 255;
            if (iv < 0) iv = 0;
            if (iv > 255) iv = 255;
            gray[x + y * w] = iv;
        }
    }

    apriltag_family_t *tf = tag36h11_create();
    tf->black_border = 1;

    apriltag_detector_t *td = apriltag_detector_create();
    apriltag_detector_add_family(td, tf);

    td->quad_decimate = 1.0;
    td->quad_sigma = 0.0;
    td->refine_edges = 1;
    td->refine_decode = 0;
    td->refine_pose = 0;
    emscripten_log(0, "ready to detect");

    image_u8_t im = { 
	   .width = w,
	   .height = h,
	   .stride = w,
	   .buf = gray,
    };

    zarray_t *detections = apriltag_detector_detect(td, &im);
    for (int i = 0; i < zarray_size(detections); i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);

        // emscripten_log(0, "detection %3d: id (%2dx%2d)-%-4d, hamming %d, goodness %8.3f, margin %8.3f\n",
        //        i, det->family->d*det->family->d, det->family->h, det->id, det->hamming, det->goodness, det->decision_margin);
        // emscripten_log(0, "%f,%f %f,%f %f,%f %f,%f",
        //         det->p[0][0], det->p[0][1], 
        //         det->p[1][0], det->p[1][1], 
        //         det->p[2][0], det->p[2][1], 
        //         det->p[3][0], det->p[3][1]
        // );
        cb(det->id,
           det->p[0][0], det->p[0][1], 
           det->p[1][0], det->p[1][1], 
           det->p[2][0], det->p[2][1], 
           det->p[3][0], det->p[3][1],
           det->H->data[0],
           det->H->data[1],
           det->H->data[2],
           det->H->data[3],
           det->H->data[4],
           det->H->data[5],
           det->H->data[6],
           det->H->data[7],
           det->H->data[8]
        );
    }
    zarray_destroy(detections);
    apriltag_detector_destroy(td);
    tag36h11_destroy(tf);
    return 0;
}
