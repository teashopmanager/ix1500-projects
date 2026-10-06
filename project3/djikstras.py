import csv

# Läser CSV filen och returnerar en graf
def load_graph(filename):

    # Skapar en tom dictionary
    graph = ()

    # Öppnar filen filename i read mode
    with open(filename, "r", encoding = "utf-8") as file:

        # Läs CSV filen som dictionarys, hoppa över blank spaces
        reader = csv.DictReader(file, skipinitialspace = True)

        # Läser rad för rad och använder start, slut, restid och linje som våra kolumner
        for row in reader:
            start = row["Start"]
            end = row["Slut"]
            travel_time = int(row["Restid"])
            line = row["Linje"]

            # Om en start station inte finns som en nod i grafen, skapa en nod
            if start not in graph:
                graph[start] = []

            # Om en slut station inte finns som en nod i grafen, skapa en nod
            if end not in graph:
                graph[end] = []

            # Lägger till kant till båda riktningarna
            graph[start].append((end, travel_time, line))
            graph[end].append((start, travel_time, line))

    return graph

def dijkstras():
    