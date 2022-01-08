% Example Connectivity Matrix - Can be Changed to Any Dimension
ConnectivityMatrix = [ 0 1 0 1 0; 1 0 0 1 1; 1 0 0 1 0; 1 1 0 0 1; 1 0 0 0 0];

% The Rest of the Code Implements the PageRank Algorithm
[rows, columns] = size(ConnectivityMatrix);

dimension = size(ConnectivityMatrix, 1);

columnsums = sum(ConnectivityMatrix, 1);

p = 0.85;

zerocolumns = find(columnsums~=0);

D = sparse( zerocolumns, zerocolumns, 1./columnsums(zerocolumns), dimension, dimension);

StochasticMatrix = ConnectivityMatrix * D;

[row, column] = find(columnsums==0);

StochasticMatrix(:, column) = 1./dimension;

Q = ones(dimension, dimension);

TransitionMatrix = p * StochasticMatrix + (1 - p) * (Q/dimension);

PageRank = ones(dimension, 1);

for i = 1:100; PageRank = TransitionMatrix * PageRank; end

% The PageRank Vector Stores the Normalized Pagerank Results for the Website
PageRank = PageRank / sum(PageRank);
