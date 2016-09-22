
Decision Trees and Random Forests

-Devendra Pratap Yadav 


We build decision trees and random forests for a insurance dataset, evaluating it for various experiments such as adding noise and tree pruning.
Dataset taken from : https://archive.ics.uci.edu/ml/datasets/Insurance+Company+Benchmark+%28COIL+2000%29 


HOW TO RUN : 

1) Compile the program by entering the following command :

   g++  -o ID3 ID3.cpp
 
2) The exceutable requires two command line arguments. The first argument should be the
   name of the file containing the dataset. The .txt file should be in the same folder
   as the executable file. The second argument should specify the experiment number,
   i.e. 1, 2, 3 or 4 ; 
  
3) Run the executable by entering the following command : 

   ./ID3  ticdata2000.txt  experiment_no
   
   ticdata2000.txt contains the dataset for creating the tree.
   
3) Press enter to print the output. Although the code has been optimized using appropriate data
   structures to minimize processing time, still some experiments may take more than a few seconds
   to run.  
   
4) Please refer to the Results and Conclusion file to see the final results of all the experiments.  



Experiments : 
1. We vary the "stopping criteria" that prevents further splitting of node. Changes in accuracy and complexity of model are observed.
2. Add noise to the dataset and evaluate the accuracy of the model along with the change in its complexity (number of nodes)
3. Perform "Reduced Error Pruning" on the tree and measure the change in accuracy of the tree.
4. Create a random forest using "Feature Bagging" approach where we select a subset of features, make multiple trees, and take majority vote for the result.


