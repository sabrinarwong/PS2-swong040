# TBA

# PS2
Extending PS1, stemming is included when creating the inverted index.

## Running PS2
Run the following commands:

```bash
git clone https://github.com/swong040/PS2-swong040.git;
cd PS2-swong040;
make; ./index;
```

## About
Included in this repo is: 
1. `porter2_stemmer/` - stemming algorithm library
2. index.txt - list of index created and number of terms in each document
3. query.txt - list of query created and number of terms in each query
4. main.h with index and query_index implementation
<!-- `main.cpp` prompting user input and `main.h` implementing the VSM  -->


## Assumptions
- terms like "U.S." are broken into two terms: u, s.

## ISSUES AS OF RIGHT NOW: 
- 1988 cannot print out relevant docs


## Contributing
`porter2_stemmer/` algorithm folder and `MakeFile` (modified) taken from [here](https://github.com/smassung/porter2_stemmer)

Sabrina Wong
swong040@ucr.edu
861195047
