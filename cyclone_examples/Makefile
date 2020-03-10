# eg1: eg1-output.dot eg1-output-cycles

# eg1-cyclone-input: eg1-input
# 	python ../src/mes-run.py eg1-input eg1-cyclone-input

# eg1-output.dot: eg1-cyclone-input
# 	../cyclone eg1-cyclone-input -edges -f eg1-output.dot

# eg1-output-cycles: eg1-cyclone-input
# 	../cyclone eg1-cyclone-input -f eg1-output-cycles

# eg1-output-traj: eg1-cyclone-input eg1-input-traj
# 	../cyclone eg1-cyclone-input -traj eg1-input-traj -f eg1-output-traj # this line hangs...

# eg1-clean:
# 	rm -rf eg1*output*

# eg1: eg1-output.dot eg1-output-cycles

.PRECIOUS: %-output.dot
.PRECIOUS: %-output-cycles
.PRECIOUS: %-cyclone-input

%-all: %-output.dot %-output-cycles
	echo > $@

%-cyclone-input: %-input
	python ../src/mes-run.py $< $@

%-output.dot: %-cyclone-input
	../cyclone $< -edges -f $@

%-output-cycles: %-cyclone-input
	../cyclone $< -f $@

clean:
	rm -rf *output*

# %-output-traj: %-cyclone-input %-input-traj
# 	../cyclone %-cyclone-input -traj %-input-traj -f %-output-traj # this line hangs...
