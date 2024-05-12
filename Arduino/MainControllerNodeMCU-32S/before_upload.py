Import("env")
import serial, time

def before_upload(source, target, env):
    print("Sending '|' before upload...\n")
    ser = serial.Serial(env.subst("$UPLOAD_PORT"))
    msg = "|\n"
    ser.write(msg.encode())
    ser.flush()
    ser.close()
    time.sleep(1)

env.AddPreAction("upload", before_upload)

