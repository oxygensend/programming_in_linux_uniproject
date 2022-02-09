import random
with open("test_input2", mode='wb') as file:
    for i in range(65536*3 ):
#       for i in range(10000):
#          file.write((1).to_bytes(2,'little'))
#      for i in range(10000):
#          file.write((2).to_bytes(2,'little'))
#      for i in range(1000):
#          file.write((4).to_bytes(2,'little'))
       for i in range(12):
           file.write((3).to_bytes(2,'little'))
       file.write((random.randint(0,65535).to_bytes(2, 'little')))
