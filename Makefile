simFDS = ./simFDS

all:
	mkdir -p build
	cd build && cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release && make

clean:
	rm -rf build 

check: test/eg5.pds
	${simFDS} test/eg5
docker:
	docker build -t discretedynamics/cyclone:0.1.0 .

docker-run:
	docker run -p 31331:8765 --name cyclone discretedynamics/cyclone:0.1.0 
# after this, go to browser, load: localhost:31331

docker-bash:
	docker exec -it cyclone /bin/bash

docker-nuke:
	docker container rm $(docker container ls -aq)

docker-prune:
	docker container prune

