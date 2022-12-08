from ftplib import FTP, all_errors

def main():
    HOST = "localhost"
    PORT = 5000     # Change this to your port 

    ftp = FTP()
    try:
        ftp.set_debuglevel(1)

        # Connect to the FTP server
        ftp.connect(HOST, PORT)
        print(ftp.login("cs317"))

        # Set passive mode
        ftp.set_pasv(True)

        # Get the current working directory
        ftp.nlst()
        
        # Retrieve a file
        ftp.retrlines("RETR dir.h")

        # Change to invalid directory
        try:
            ftp.cwd("random")
        except all_errors as should_fail:
            print("Failed succesfully:", should_fail)

        # Retrieve a non-existent file
        try:
            ftp.retrlines("RETR hlkljh.g")
        except all_errors as should_fail:
            print("Failed succesfully:", should_fail)

        # Test CDUP
        ftp.cwd("test")
        ftp.cwd("..")
        ftp.retrlines("RETR usage.h")

        ftp.quit()
        
    except all_errors as e:
        print('FTP error:', e)

if __name__ == "__main__":
    main()