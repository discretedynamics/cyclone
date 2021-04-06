SIMFDS = ../../build/simFDS

check:
	@${SIMFDS} eg1
	@./test-uniqueness.sh eg1
	@${SIMFDS} eg2
	@./test-uniqueness.sh eg2
	@${SIMFDS} eg3
	@./test-uniqueness.sh eg3
	@${SIMFDS} eg4
	@./test-uniqueness.sh eg4
	@${SIMFDS} eg5
	@./test-uniqueness.sh eg5

# check: eg5.test-is-ok
# %.test-is-ok:%
# 	@${SIMFDS} %
# 	@./test-uniqueness.sh %

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
