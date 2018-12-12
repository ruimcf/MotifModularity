import sys


def load_tables(dataFilePath):
    with open(dataFilePath) as dataFile:
        header = next(dataFile)
        index = 1
        titles_table = {}
        directors_table = {}
        actors_table = {}
        for line in dataFile:
            data = line.split("\t")
            rank = data[0]
            title = data[1].strip()
            genre = data[2]
            description = data[3]
            director = data[4].strip()
            actors = data[5].split(",")
            year = data[6]
            runtime = data[7]
            rating = data[8]
            votes = data[9]
            revenue = data[10]
            metascore = data[11]

            if title not in titles_table:
                titles_table[title] = {}
                titles_table[title]["id"] = str(index)
                titles_table[title]["actors"] = []
                titles_table[title]["director"] = ""
                index += 1

            if director not in directors_table:
                directors_table[director] = {}
                directors_table[director]["id"] = str(index)
                directors_table[director]["titles"] = []
                index += 1
            directors_table[director]["titles"].append(title)
            titles_table[title]["director"] = director
            
            for actor in actors:
                actor = actor.strip()
                if actor not in actors_table:
                    actors_table[actor] = {}
                    actors_table[actor]["id"] = str(index)
                    index += 1
                    actors_table[actor]["titles"] = []
                actors_table[actor]["titles"].append(title)
                titles_table[title]["actors"].append(actor)

        return (titles_table, directors_table, actors_table)
    
    
def writeEdgesToFile(targetFilePath, edgesList):
    with open(targetFilePath, 'w') as targetFile:
        targetFile.write("source\ttarget\n")
        for node1, node2 in edgesList:
            targetFile.write(node1+"\t"+node2+"\n")
 

def writePartitionToFile(targetFilePath, partitionList):
    with open(targetFilePath, 'w') as targetFile:
        targetFile.write("nodeID\tcommunity\n")
        for node, community in partitionList:
            targetFile.write(node+"\t"+community+"\n")

def parse_table():
    dataFile = "IMDB-Movie-Data.tsv"
    newFile = "new_IMDB-Movie-Data.tsv"
    partitionFile = "IMDB_real_partition.tsv"
    titles_table, directors_table, actors_table = load_tables(dataFile)
    
    selected_actors = []
    selected_titles = []
    selected_directors = []
    for actor, items in actors_table.items():
        titles = items["titles"]
        if len(titles) > 5:
            valid_titles_for_actor = []
            directors_in_valid_titles = []
            for title in titles:
                director = titles_table[title]["director"]
                # ONLY ADD TITLES THAT DO NOT SHARE DIRECTORS
                if director not in selected_directors and director not in directors_in_valid_titles:
                    valid_titles_for_actor.append(title)
                    directors_in_valid_titles.append(director)
            # ONLY ADD ACTOR THAT WILL HAVE MORE THAN 2 TITLE EDGES
            if len(valid_titles_for_actor) >= 2:
                if actors_table[actor]["id"] == "781":
                    print "added node 781, titles", selected_titles
                selected_titles += valid_titles_for_actor
                selected_directors += directors_in_valid_titles
                selected_actors.append(actor)
    print "number of titles", len(selected_titles)
    print "number of actors", len(selected_actors)
    print "mumber of directors", len(selected_directors)

    raw_edge_list = []
    raw_partition_list = []
    raw_writen_titles = []
    for actor in selected_actors:
        actor_id = actors_table[actor]["id"]
        raw_partition_list.append((actor_id, "1"))
        if actor_id == "781":
            print "781 titles", actors_table[actor]["titles"]
        for title in actors_table[actor]["titles"]:
            if title in selected_titles:
                director = titles_table[title]["director"]

                director_id = directors_table[director]["id"]
                title_id = titles_table[title]["id"]

                raw_edge_list.append((actor_id, title_id))
                if actor_id == "781":
                    print "added edge with", actor_id, title_id
                raw_edge_list.append((title_id, director_id))
                

                if title_id not in raw_writen_titles:
                    raw_partition_list.append((title_id, "2"))
                    raw_writen_titles.append(title_id)
                raw_partition_list.append((director_id, "3"))
            else:
               if actor_id == "781": 
                   print "781 does not have", title, "in selected"

    normalized_edge_list = []
    id_mapping = {}
    index = 1
    for node1, node2 in raw_edge_list:
        if node1 not in id_mapping:
            id_mapping[node1] = str(index)
            index += 1

        if node2 not in id_mapping:
            id_mapping[node2] = str(index)
            index += 1

        normalized_edge_list.append((id_mapping[node1], id_mapping[node2]))
    
    # for node, partition in raw_partition_list:
        # print "node, partition", node, partition
        # print "mapping", id_mapping[node]
    normalized_partition_list = [(id_mapping[node], partition) for node, partition in raw_partition_list]

    writeEdgesToFile(newFile, normalized_edge_list)
    writePartitionToFile(partitionFile, normalized_partition_list)


parse_table()
print "Done."
