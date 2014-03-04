port = 5556

((ARGV[0].to_i)..(ARGV[1].to_i)).each { |house|
	system("/usr/bin/time -v ./bin/house #{house} #{ARGV[2]} #{port} > /dev/null 2> #{ARGV[3]}/#{house}.out &")
}
