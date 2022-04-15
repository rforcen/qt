# models from   // https://github.com/DigitalGlobe/gbdx-caffe/tree/master/work/input/model
# convert to resource
rsc: 
	rm -fr mnistmodel.o
	ld -r -b binary -o mnistmodel.o lenet_iter_10000.caffemodel lenet.prototxt
	nm mnistmodel.o


