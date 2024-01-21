import csv

filename = "DATALOG.txt"
output = "output.csv"

outputFile = open(output, "a+", newline='')

file = open(filename, "r")

filetext = file.read()

processing = filetext.split("\n")
newProcessing = []

formatData = []
formatData.append(["Date", "Time", "Control Humidity", "Experiment Humidity", "Control Temperature", "Experiment Temperature", "Control Soil", "Experiment Soil"])

for i in range(0, len(processing)):
    newProcessing.append(processing[i].split(" "))
    
for i in range(0, len(newProcessing)):
    newProcessing[i][0] = newProcessing[i][0][1:]
    newProcessing[i][1] = newProcessing[i][1][:-1]

for i in range(0, len(newProcessing)):

    date = newProcessing[i][0]
    time = newProcessing[i][1]

    if newProcessing[i][2] == "CH":
        humC = newProcessing[i][3]

    elif newProcessing[i][2] == "EH":
        humE = newProcessing[i][3]

    elif newProcessing[i][2] == "CT":
        tempC = newProcessing[i][3]
    
    elif newProcessing[i][2] == "ET":
        tempE = newProcessing[i][3]
    
    elif newProcessing[i][2] == "CS":
        soilC = newProcessing[i][3]

    elif newProcessing[i][2] == "ES":
        soilE = newProcessing[i][3]

    if i != 0:
        if i%7 == 0:
            formatData.append([date, time, humC, humE, tempC, tempE, soilC, soilE])

with outputFile:    
    write = csv.writer(outputFile)
    write.writerows(formatData)