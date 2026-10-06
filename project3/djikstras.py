import csv
import heapq

# Läser CSV filen och returnerar en graf
def load_graph(filename):

    # Skapar en tom dictionary
    graph = {}

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

def dijkstras(graph, start, end):

    distances = {}

    for station in graph:
        distances[station] = float("inf")

    distances[start] = 0

    previous = {}

    priority_queue = [(0, start)]

    while priority_queue:

        # Hämtar stationen med kortast restid
        current_distance, current_station = heapq.heappop(priority_queue)

        # Ignorera gamla/sämre vägar
        if current_distance > distances[current_station]:
            continue

        # Vi har nått vår slut station
        if current_station == end:
            break

        # Gå igenom alla grannar till nuvarande station (alla kanter)
        for neighbour, travel_time, line in graph[current_station]:

            # Beräknar restiden om vi åker via current_station
            new_distance = current_distance + travel_time

            # Om denna väg är kortare än den tidigare bästa väg till neighbour
            if new_distance < distances[neighbour]:

                # Uppdaterar kortaste restiden
                distances[neighbour] = new_distance

                # Sparar att vi kom till neighbour från current_station
                previous[neighbour] = current_station

                # Lägg den nya vägen i priority_queue
                heapq.heappush(priority_queue, (new_distance, neighbour))

    # Om målstationsn fortfarande har oändligt (inf) avstånd, finns ingen väg mellan start och end
    if distances[end] == float("inf"):
        return float("inf"), []

    # Återskapar kortaste vägen

    path = []

    current_station = end

    # Börjar på slutstationen och följer vägen tillbaka

    while current_station != start:
        path.append(current_station)
        current_station = previous[current_station]

    # Lägg till startstationen
    path.append(start)

    # Vänder på hela listan så den blir åt rätt håll
    path.reverse()

    # Returnera kortaste restid och själva vägen
    return distances[end], path

def main():

    graph = load_graph("sl.csv")

    start = "Upplands Väsby"

    end = "Liljeholmen"

    distance, path = dijkstras(graph, start, end)

    if distance == float("inf"):
        print("Det finns ingen väg mellan stationerna")

    else:
        print("Kortaste restiden:", distance, "minuter")
        print("Kortaste väg:")
        print(" -> ".join(path))


if __name__ == "__main__":
    main()

