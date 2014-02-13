#!/usr/bin/python
import mechanize
import time

def main():
    while True:
        uname="bkd.online"
        br=mechanize.Browser()
        try:
            br.open("https://internet.iitb.ac.in")
        except Exception as e:
            time.sleep(5)
            print "Trying again"
            continue

        # if it finds you logged in, it logs you out and logs back.
        # It saves the gap that might come between automatic logout and script's next iteration
        if br.geturl()=="https://internet.iitb.ac.in/logout.php":
            br.select_form("auth")
            br.submit() # logout

        # get password
        fp=open(".ldap_pass","r")
        passwd=fp.readline().strip();
        fp.close()

        try:
            br.open("https://internet.iitb.ac.in")
        except Exception as e:
            time.sleep(5)
            print "Trying again"
            continue
        br.select_form("auth")
        br["uname"]=uname
        br["passwd"]=passwd
        br.submit()
        br.close()

        print "Successful iteration!"
        time.sleep(5*60)


if __name__=="__main__":
    main()
