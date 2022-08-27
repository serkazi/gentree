## Generate Random Trees 

#### Workflow 1 (current)
0. Given the number of nodes, `n`
1. Generate a random bracket balanced sequence of length `n-1`
2. Wrap it inside `(` and `)` 

#### Worflow 2 (not implemented)
1. Generate a random binary tree
2. Use natural correspondence to convert it to an ordinal tree

#### TODO
- [ ] Use Factory pattern to supply the actual implementations of the interfaces
- [ ] Further refactorings and abstraction
- [ ] Add logging

#### References
* http://www.cs.otago.ac.nz/staffpriv/mike/Papers/RandomGeneration/RandomBinaryTrees.pdf
* https://watermark.silverchair.com/230161.pdf

