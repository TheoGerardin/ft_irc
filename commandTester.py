import subprocess
import time
import threading
import os
import sys

def read_output(process):
    while True:
        output = process.stdout.readline()
        if output == '' and process.poll() is not None:
            break
        if output:
            print(output.strip())

process = subprocess.Popen(['nc', 'localhost', '5555'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

thread = threading.Thread(target=read_output, args=(process,))
thread.start()

time.sleep(1)

try:
    commands = ['JOIN #test\n', 'PASS irc\n', 'JOIN #test\n', 'PRIVMSG #test goodmessage\n', 
                'PART #test\n', 'PRIVMSG #test badmessage\n',
                'JOIN #chan\n', 'PRIVMSG #chan !hello\n', 'PRIVMSG #chan !leave\n']
    for command in commands:
        process.stdin.write(command)
        process.stdin.flush()
        time.sleep(0.5) 
except BrokenPipeError:
    print("nc closed prematurly.")

os.dup2(process.stdin.fileno(), sys.stdin.fileno())
os.dup2(process.stdout.fileno(), sys.stdout.fileno())
os.dup2(process.stderr.fileno(), sys.stderr.fileno())

thread.join()