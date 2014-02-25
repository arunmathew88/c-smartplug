# unused broker: 10.129.34.88
# unused house servers: 10.129.34.67 (10.129.34.38 not working)
brokers = ["10.129.34.64", "10.129.34.40"]
houses = ["10.129.34.57", "10.129.34.58", "10.129.34.67", "10.129.34.61", "10.129.34.36", "10.129.34.66"]
filename = ["sorted.csv", "20houses.csv", "10houses.csv"]

system("make clean")
brokers.each.with_index { |b, i|
	system("sshpass -p synerg ssh synerg@#{b} \"rm -rf /home/synerg/c-smartplug/; pkill broker\"")
	system("sshpass -p synerg scp -r ../c-smartplug/ synerg@#{b}:/home/synerg/")
	system("sshpass -p synerg ssh synerg@#{b} \"cd c-smartplug; make; ./bin/broker #{(2**(2-i))*10} /media/ext_disk/#{filename[i]} 1000000 > out 2>&1 & \"")
}

houses.each.with_index{ |h, i|
	system("sshpass -p synerg ssh synerg@#{h} \"rm -rf /home/synerg/c-smartplug/; pkill house\"")
	system("sshpass -p synerg scp -r ../c-smartplug/ synerg@#{h}:/home/synerg/")
	system("sshpass -p synerg ssh synerg@#{h} \"cd c-smartplug; make; ruby script/houses.rb #{(i%4)*10}  #{(i%4)*10+9} #{brokers[i/4]} \"")
}
