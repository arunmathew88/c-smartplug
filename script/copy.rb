brokers = []
houses = []
filename = []
username = ""
password = ""
eval File.open('script/common.rb').read
require 'shellwords'

system("make clean")
brokers.each.with_index { |b, i|
	system("sshpass -p #{password.shellescape} ssh #{username}@#{b} \"rm -rf /home/#{username}/c-smartplug/; pkill broker\"")
	system("sshpass -p #{password.shellescape} scp -r ../c-smartplug/ #{username}@#{b}:/home/#{username}/")
	system("sshpass -p #{password.shellescape} ssh #{username}@#{b} \"cd c-smartplug; make; \"")
}

houses.each.with_index{ |h, i|
	system("sshpass -p #{password.shellescape} ssh #{username}@#{h} \"rm -rf /home/#{username}/c-smartplug/; pkill house\"")
	system("sshpass -p #{password.shellescape} scp -r ../c-smartplug/ #{username}@#{h}:/home/#{username}/")
	system("sshpass -p #{password.shellescape} ssh #{username}@#{h} \"cd c-smartplug; make; \"")
}

