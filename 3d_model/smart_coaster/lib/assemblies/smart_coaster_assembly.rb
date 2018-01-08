class SmartCoasterAssembly < CrystalScad::Assembly
		
	# Assemblies are used to show how different parts interact on your design.
 
	# Skip generation of the 'output' method for this assembly.
	# (will still generate 'show')
	skip :output

	def part(show)
		# Create a test cube
		coaster_bottom = CoasterBottom.new

		# We're calling the show method on both cubes
		res = coaster_bottom.show

		res
	end

end
