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

def constrained_djikstras(graph, start, end, max_transfers=None, forbidden_line=None, required_station=None):

    # Ett state består av: (station, nuvarande linje, antal byten, har besökt required station)
    # Exempel ("Slussen", "Gröna", 1, True)

    start_visited_required = (required_station is None or start == required_station)

    start_state = (start, None, 0, start_visited_required)

    # Kortaste  kända restid till varje state

    distances = {start_state, 0}

    # Används för att återskapa vägen
    previous = {}

    # Priority queue: (restid, station, linje, antal byten, besökt required state)
    priority_queue = [(0, start, None, 0, start_visited_required)]

    final_state = None

    while priority_queue:
        (current_distance, current_station, current_line, transfers, visited_required) = heapq.heappop(priority_queue)

        current_state = (current_station, current_line, transfers, visited_required)

        # Ignorera en gammal/sämre väg
        if current_distance > distances[current_state]:
            continue

        # Vi får bara avsluta programmet om: 1, Vi når slutstationen 2, required_station är avstängd eller har besökts
        if current_station == end and visited_required:
            final_state = current_state
            break

        # Undersöker alla grannar (kanter) från nuvarande station
        for neighbour, travel_time, line in grapf[current_station]:
            # Constraint 1: Förbjuden linje

            if forbidden_line is not None and line == forbidden_line:
                continue

            # Constraint 2: Max antal byten
            new_transfers = transfers

            # Om vi har gjort för många byten, kasta den vägen

            if (max_transfers is not None and new_transfers > max_transfers):
                continue

            # Constraint 3: Måste besöka station
            
            new_visited_required = visited_required

            if (required_station is not None and neighbour == required_station):
                new_visited_required = True

            # Nytt state efter att kanten har använts
            new_state = (neighbour, line, new_transfers, new_visited_required)

            new_distance = current_distance + travel_time

            if (new_state not in distances or new_distance < distances[new_state]):
                distances[new_state] = new_distance
                previous[new_state] = current_state

                heapq.heappush(priority_queue, (new_distance, neighbour, line, new_transfers, new_visited_required))

    # Om ingen giltig väg hittas
    if final_state is None:
        return float ("inf"), [], 0

    # Återskapa vägen
    path = []

    current_state = final_state

    while current_state != start_state:
        station, line, transfers, visited_required = current_state

        path.append(station)

        current_state = previous[current_state]

    path.append(start)

    path.reverse()

    # Antal byten finns i final_state

    total_transfers = final_state[2]

    return distances[final_state], path, total_transfers

#def reconstruct_path():

def main():

    graph = load_graph("sl.csv")

    start = "Upplands väsby"

    end = "Liljeholmen"

    distance, path = dijkstras(graph, start, end)

    if distance == float("inf"):
        print("Det finns ingen väg mellan stationerna")

    else:
        print("Kortaste restiden:", distance, "minuter")
        print("Kortaste väg:")
        print(" -> ".join(path))

def main_con():
    graph = load_graph("sl.csv")

    start = "Upplands väsby"

    end = "Liljeholmen"

    distance, path, transfers = constrained_djikstras(graph, start, end, max_transfers=1, forbidden_line=None, required_station=None)

    if distance == float("inf"):
         print("Det finns ingen väg mellan stationerna")
        
    else:
        print("Kortaste restiden:", distance, "minuter")
        print("Kortaste väg:")
        print(" -> ".join(path))        


if __name__ == "__main__":
    main()

