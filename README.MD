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
<!-- 2. index.txt - list of index created and number of terms in each document
3. query.txt - list of query created and number of terms in each query -->
2. `results_file.txt` with query execution
<!-- 4. main.h with index and query_index implementation -->
3. `main.cpp` prompting user input and `main.h` implementing the VSM and query input


## Assumptions
- terms like "U.S." are broken into two terms: u, s.


## Contributing
`porter2_stemmer/` algorithm folder and `MakeFile` (modified) taken from [here](https://github.com/smassung/porter2_stemmer)

Sabrina Wong 

swong040@ucr.edu

861195047
