import sys


def parse_table():
    filename = "IMDB-Movie-Data.tsv"
    newFile = "new_IMDB-Movie-Data.tsv"
    with open(filename) as f:
        with open(newFile, 'w') as newF:
            with open("IMDB_mapping.tsv", 'w') as mappingFile:
                with open("IMDB_real_partition", 'w') as realPartitionFile:
                    header = next(f)
                    newF.write("source\ttarget\n")
                    mappingFile.write("Id\ttype\n")
                    index = 1
                    titles = {}
                    actors = {}
                    directors = {}
                    numberOfTitles = 0
                    for line in f:
                        numberOfTitles += 1
                        if(numberOfTitles > 50):
                            return

                        titleId, directorId = 0, 0
                        data = line.split("\t")
                        rank = data[0]
                        title = data[1]
                        genre = data[2]
                        description = data[3]
                        director = data[4]
                        actorsList = data[5]
                        year = data[6]
                        runtime = data[7]
                        rating = data[8]
                        votes = data[9]
                        revenue = data[10]
                        metascore = data[11]

                        if title not in titles:
                            titles[title] = index
                            mappingFile.write(str(index)+"\t2\n")
                            realPartitionFile.write("2\n")
                            index += 1
                        titleId = titles[title]

                        if director not in directors:
                            directors[director] = index
                            mappingFile.write(str(index)+"\t3\n")
                            realPartitionFile.write("3\n")
                            index += 1
                        directorId = directors[director]

                        for actor in actorsList.split(","):
                            if actor not in actors:
                                actors[actor] = index
                                mappingFile.write(str(index)+"\t1\n")
                                realPartitionFile.write("1\n")
                                index += 1
                            actorId = actors[actor]

                            newF.write(str(titleId)+"\t"+str(actorId)+"\n")

                        newF.write(str(titleId)+"\t"+str(directorId)+"\n")


parse_table()
print "Done."
