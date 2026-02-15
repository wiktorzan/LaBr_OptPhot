from multiprocessing.dummy import Pool as PoolThread
from random import randint
from time import sleep
from os import system
from numpy import linspace, arange

numberOfThreads = 4

def fixed_length(n, width=3):
    if n >= 0:
        return "{:0{w}d}".format(n, w=width)
    else:
        return "-{:0{w}d}".format(abs(n), w=width-1)

def sim(point):
    i, j = point

    with open("grid.mac", "w",) as file:
            file.write("/gps/particle gamma\n")
            file.write("/gps/number 1\n")
            file.write(f"/gps/pos/centre {i} {j} -80 mm\n")
            file.write("/gps/energy 0.5 MeV\n")
            file.write("/gps/direction 0 0 1\n")
            file.write("/run/beamOn 1000\n")
    sleep(1)
    
    system("~/SABAT/LaBr/build/LaBr3_V2 grid.mac " + fixed_length(i) + " " + fixed_length(j))
    sleep(1)
    # system("mv Out* scan_{0}_{1}.root".format(i, j))
    # system("mv Out* scan_"+ fixed_length(i) +"_"+ fixed_length(j) +".root")



processNumbersToSim = 20
numbers = set()
while len(numbers) < processNumbersToSim:
    numbers.add(randint(0, processNumbersToSim+1))



# npoints = 10
# size = 2.54/2 #cm
points_list = []

# for i in linspace(-size, size, npoints):
#     for j in linspace(-size, size, npoints):
#         points_list.append((i, j))
# 

max_dist = 4 #mm
for i in arange(-max_dist, max_dist+0.01, 1):
    for j in arange(-max_dist, max_dist+0.01, 1):
        points_list.append((int(i), int(j)))


system("bash ~/SABAT/LaBr/scripts/send_notification.sh 'start symulacji optycznej laptop'")
pool = PoolThread(numberOfThreads)
results = pool.map(sim, points_list)
system("bash ~/SABAT/LaBr/scripts/send_notification.sh 'koniec symulacji optycznej laptop'")