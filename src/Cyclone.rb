require 'json'
require 'open3'

class Cyclone	
	def initialize(json)
		@model_name = ''
		@simulation_name = ''
		@no_of_variables = 0
		@variable_names = []
		@no_of_states = []
		@speed_of_variables = []
		@json_data = json
		self.j2i_parse_json()
	end

	def j2i_parse_json()
		@parsed = JSON.parse(@json_data)
		@model_name = @parsed["description"]
		@variables = @parsed["variables"]
		for i in 0...@variables.length
			@variable_names.push(@variables[i]["id"])
			@no_of_states.push(@variables[i]["states"].length)
			@speed_of_variables.push(@variables[i]["speed"])
		end
		@no_of_variables = @variables.length

		@state_transition_tables = ''
		@update_rules = @parsed["updateRules"]
		for i in 0...@update_rules.length()
			@target = @update_rules[i]["target"]
			@state_transition_tables += "STATE TRANSITION TABLE for " + @target + ":\n"
			@input_variables = @update_rules[i]["functions"][0]["inputVariables"]
			@state_transition_tables += @input_variables.join(" ") + " " + @input_variables[@input_variables.length - 1]
			@state_transition_tables += "\n"
			@transition_table = @update_rules[i]["functions"][0]["transitionTable"]
			for i in 0...@transition_table.length()
				@state_transition_tables += String(@transition_table[i][1]) + "\n"
			end
			@state_transition_tables += "\n\n"
		end
	end

	def j2i_get_input()
		@formatted_input = ''
		@formatted_input += "MODEL NAME: model-for-cyclone \n"
		@formatted_input += "SIMULATION NAME: test-simulation\n"		
		@formatted_input += "NUMBER OF VARIABLES: " + String(@no_of_variables) + "\n"
		@formatted_input += "VARIABLE NAMES:"
		for vn in @variable_names
			@formatted_input += " " + vn
		end
		@formatted_input += "\n"
		@formatted_input += "NUMBER OF STATES:"
		for ns in @no_of_states
			@formatted_input += " " + String(ns)
		end
		@formatted_input += "\n"
		@formatted_input += "SPEED OF VARIABLES:"
		for sv in @speed_of_variables
			@formatted_input += " " + String(sv)
		end
		@formatted_input += "\n\n"
		@formatted_input += @state_transition_tables
		return @formatted_input
	end

	def clean()
		File.delete(@cyclone_input_file) if File.exist?(@cyclone_input_file)
		File.delete(@cyclone_output_file) if File.exist?(@cyclone_output_file)
	end

	def o2j_trajectory(output_file)
		@output = [{}]
		f = File.open(output_file, "r")
		line = f.gets
		@output[0]["modelName"] = line.split(":")[1].strip
		line = f.gets
		line = f.gets
		@output[0]["simulationName"] = line.split(":")[1].strip
		line = f.gets
		line = f.gets
		@output[0]["sizeOfStateSpace"] = Integer(line.split(":")[1].strip)
		line = f.gets
		line = f.gets
		@output[0]["numberOfCycles"] = Integer(line.split(":")[1].strip)
		line = f.gets
		line = f.gets
		@output[0]["component"] = []
		while line != "DONE\n" do
			component = {}
			component["id"] = Integer(line.split(" ")[1].split(":")[0])
			line = f.gets
			component["size"] = Integer(line.split("=")[1].split("(")[0].strip)
			component["percent"] = Float(line.split("=")[1].split("(")[1].split(" ")[0])
			line = f.gets
			component["dynamics"] = line.split(":")[0]
			line = f.gets
			component["length"] = Integer(line.split("=")[1].strip)
			component["states"] = []
			for i in 0...component["length"]
				line = f.gets
				state = line.split("=")[1].strip
				state = state[1...(state.length() - 1)].strip.split(' ').map {|x| x.to_i}
				component["states"].push(state)
			end
			@output[0]["component"].push(component)
			line = f.gets
			line = f.gets
		end		
		f.close()
		@final = {}
		@final["output"] = @output
		@final_json = JSON.pretty_generate(@final)
		clean()
	end

	def o2j_edges(output_file)
		@output = {}
		f = File.open(output_file, "r")
		@node = []
		@connection = []
		while (line = f.gets) do
			if line.include? ";"
				node_entry = {}
				node_entry["id"] = line.split("[")[0].strip
				temp = line.split("=")[1]
				node_entry["label"] = temp[1...(temp.length()-4)]
				@node.push(node_entry)
			elsif line.include? "->"
				connection_entry = []
				connection_entry.push(line.split("->")[0].strip)
				connection_entry.push(line.split("->")[1].strip)
				connection_entry.push("")
				connection_entry.push("")
				@connection.push(connection_entry)
			end
		end
		@output["type"] = "AnnotatedGraph"
		@output["nodes"] = @node
		@output["edges"] = @connection
		f.close()
		@final_json = JSON.pretty_generate(@output)
		clean()		
	end
	
	def get_final_json()
		return @final_json
	end

	def run()
		#Dir.chdir ENV["CODE_HOME"] + "src/"
		@in = j2i_get_input()
		@cyclone_input_file = "cyclone_input.txt"
		@cyclone_output_file = "cyclone_output.txt"
		f = File.open(@cyclone_input_file, "w")
		f.write(@in)
		f.close()
		system("cyclone " + @cyclone_input_file + " -table -edges -f " + @cyclone_output_file)
		o2j_edges(@cyclone_output_file)
		return get_final_json()
	end
end

input_file = ARGV[0].dup
f = File.open(input_file)
js = ''
f.each_line do |line|
	js += line
end
f.close()
ex_file = ''
@test = Cyclone.new(js)
result = @test.run()
f = File.open("output.txt", "w")
f.write(result)
f.close()
