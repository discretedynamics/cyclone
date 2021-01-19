# input: eg1.input  in PDS format
# creates: eg1.tables
#          eg1.dot
#          eg1.cycles

.PRECIOUS: %.dot
.PRECIOUS: %.cycles
.PRECIOUS: %.tables

%-all: %.dot %.cycles
	echo > $@

%.tables: %.input
	python ../src/pds-to-tables.py $< $@

%.dot: %.tables
	../cyclone $< -edges -f $@

%.cycles: %.tables
	../cyclone $< -f $@

clean:
	rm -rf *.dot *.cycles *.tables *-all

# todo: get trajectories working
# %-output-traj: %-cyclone-input %-input-traj
# 	../cyclone %-cyclone-input -traj %-input-traj -f %-output-traj # this line hangs...
