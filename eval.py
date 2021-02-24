#!/bin/python

# To run this file you need the following libraries:
# pandas - For dataframe handling
# seaborn - For plotting
# matplotlib - To control plotting done by seaborn
#
# You can use the given requirements.txt file to install through pip with the command:
# $> python -m pip install -r requirements.txt
#
# If you don't have pip installed, you can install it on Linux-like operating systems as so:
# sudo apt install python3-pip
#
# Alternatively on IOS systems, brew should install pip with python3:
# brew install python3
#

import csv

import matplotlib.pyplot as plt

from seaborn import lineplot
from pandas import DataFrame


def parse_data(files):
    #df = []
    add = []
    union = []
    intersection = []
    difference = []

    # Open each input file and read its contents
    for f in files:
        with open(f, "r") as fp:
            reader = csv.reader(fp, delimiter=" ")

            # Parse each line of the file
            for line in reader:
                n = int(line[0] + line [2]) // 2
                t = line[1]
                t_add = int(line[1] + line[3]) // 2
                t_un = line[3]
                t_in = line[4]
                t_dif = line[5]

                # Get the algorithm and order from the filename
                #alg, order = f.split("_")
                #order = order.split(".")[0]
                alg = f.split(".")[0]

                #df.append((int(n), int(t), alg, order))
                add.append((int(n), int(t_add), alg))
                union.append((int(n), int(t_un), alg))
                intersection.append((int(n), int(t_in), alg))
                difference.append((int(n), int(t_dif), alg))

    # Put the data into a pandas DataFrame
    #df = DataFrame(df, columns=["n", "t(usec)", "algorithm", "order"])
    add = DataFrame(add, columns=["n", "t(usec)", "algorithm"])
    union = DataFrame(union, colums = ["n", "t(usec)", "algorithm"])
    intersection = Dataframe(intersection, columns = ["n", "t(usec)", "algorithm"])
    difference = DataFrame(intersection, columns = ["n", "t(usec)", "algorithm"])

    return add, union, intersection, difference


if __name__ == "__main__":
    add, union, intersection, difference = parse_data([
        "array.data",
        "list.data",
        "list_simple.data"
    ])

    # Plot the data as lines
    #lineplot(data=df, x="n", y="t(usec)", hue="algorithm", style="order")
    lineplot(data=add, x="n", y="t(usec)", hue="algorithm")
    lineplot(data=union, x="n", y="t(usec)", hue="algorithm")
    lineplot(data=intersection, x="n", y="t(usec)", hue="algorithm")
    lineplot(data=difference, x="n", y="t(usec)", hue="algorithm")
    plt.show()
