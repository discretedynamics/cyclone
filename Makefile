all:
	mkdir -p build
	cd build && cmake -S ../src -B . && make

clean:
	rm -rf build 

docker:
	docker build -t discretedynamics/cyclone:0.1.0 .

docker-run:
	docker run -p 31331:8765 --name cyclone discretedynamics/cyclone:0.1.0 

docker-bash:
	docker exec -it cyclone /bin/bash

docker-nuke:
	docker container rm $(docker container ls -aq)

docker-prune:
	docker container prune
