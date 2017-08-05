all:
	emcc -s RESERVED_FUNCTION_POINTERS=20 -s ALLOW_MEMORY_GROWTH=1 -I. -O2 \
	    april-api.c apriltag.c common/workerpool.c common/matd.c common/image_u8.c \
	    common/pnm.c common/homography.c common/g2d.c common/svd22.c tag36h11.c \
	    apriltag_quad_thresh.c -o html/apriltag.js -s EXPORTED_FUNCTIONS="['_detect']"

clean:
	rm -f html/apriltag.js html/apriltag.js.mem

