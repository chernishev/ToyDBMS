#!/usr/bin/env python

# to be executed from the root dataset directory
# creates catalog.txt in the current working directory
# creates $table.$column.hist files in the tables directory

# every histogram will be equi-width with 10 buckets by default

import os
import csv
from collections import Counter

# get all table files
tables = [f for f in os.listdir('tables') if os.path.isfile('tables/' + f) and f.endswith('.csv')]

types = {'i': 'INT', 's': 'STR'}

def parse_value(header, row):
    return [int(v) if h['type'] == 'INT' else v for h, v in zip(header, row)]

def generate_histogram(filename, freq):
    N = 10 # number of buckets
    vals = sorted(freq.keys())

    bucket_size = max(1, len(vals) / N)
    leftover = len(vals) % N

    sizes = [bucket_size + (len(vals) >= N)] * leftover + [bucket_size] * (min(len(vals), N) - leftover)

    i = iter(vals)
    buckets = [[next(i) for _ in range(cur)] for cur in sizes]
    with open(filename, 'w') as f:
        for bucket in buckets:
            f.write('%s %s %d\n' % (bucket[0], bucket[-1], sum(map(lambda v: freq[v], bucket))))

catalog = open('catalog.txt', 'w')

def print_header(table, header, num_elements):
    catalog.write("%s %d\n" % (table, num_elements))
    for col in header:
        unique = sum(col['counter'].values()) != 0 and col['counter'].most_common(1)[0][1] == 1
        catalog.write('    %s %s %s %s %s %s\n' % (
            col['name'],
            col['type'],
            col['order'],
            'UNIQUE' if unique else "NOTUNIQUE",
            col['min'],
            col['max'],
        ))


for filename in tables:
    table = filename.split('.')[0]
    with open('tables/' + filename, 'r') as f:
        reader = csv.reader(f)
        header = reader.next()
        header = [{'name': name, 'type': types[t], 'order': 'UNKNOWN', 'counter': Counter(), 'min': 0, 'max': 0}
                  for t, name in (field.split('_') for field in header)]
        vals = [[] for i in range(len(header))]
        num_elements = 0
        try:
            row = parse_value(header, reader.next())
            num_elements += 1
            for i, v in enumerate(row):
                header[i]['counter'][v] += 1
                header[i]['min'] = v
                header[i]['max'] = v
        except StopIteration:
            print_header(table, header, num_elements)
            continue
        prev = row
        for row in reader:
            num_elements += 1
            row = parse_value(header, row)
            for i, v in enumerate(row):
                header[i]['counter'][v] += 1
                # update min-max
                header[i]['min'] = min(header[i]['min'], v)
                header[i]['max'] = max(header[i]['max'], v)
                # update sort order
                if header[i]['order'] == 'UNKNOWN':
                    if v < prev[i]: header[i]['order'] = 'DESC'
                    if v > prev[i]: header[i]['order'] = 'ASC'
                elif header[i]['order'] == 'DESC':
                    if v > prev[i]: header[i]['order'] = 'UNSORTED'
                elif header[i]['order'] == 'ASC':
                    if v < prev[i]: header[i]['order'] = 'UNSORTED'
            prev = row
        print_header(table, header, num_elements)
        for col, v in zip(header, vals):
            generate_histogram('tables/' + table + '.' + col['name'] + '.hist', col['counter'])
