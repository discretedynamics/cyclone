SIMFDS = ../../build/simFDS

eg5: eg5-statespace.dot

eg-10-vars-statespace.dot: eg-10-vars.pds
	$(SIMFDS) eg-10-vars
#	dot -Tpng -o eg-10-vars.png eg-10-vars-statespace.dot

eg5-statespace.dot: eg5.pds
	$(SIMFDS) eg5

ym-bool-model:
	$(SIMFDS) ym-bool-model
	dot -Tpng -o ym-bool-model.png ym-bool-model-statespace.dot
	open ym-bool-model.png

clean:
	rm -rf *.txt *.dot *.png
