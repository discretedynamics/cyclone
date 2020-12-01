all:
	mkdir -p build
	cd build; cmake ..; make

clean:
	rm -rf build build-1 build-old cmake-builld*
	rm foo-output*

docker:
	docker build -t discretedynamics/cyclone:0.1.0 .

docker-run:
	docker run --name cyclone discretedynamics/cyclone:0.1.0 

docker-bash:
	docker exec -it cyclone /bin/bash

docker-nuke:
	docker container rm $(docker container ls -aq)

docker-prune:
	docker container prune
