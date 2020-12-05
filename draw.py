import matplotlib.pyplot as plt

def draw_scatter():
        filename='addresses-locality.txt'
        X=[]
        Y=[]
        X=range(10000)

        with open(filename,'r') as f:
            lines=f.readlines()
            for line in lines: 
                Y.append(line)
  
        plt.scatter(X,Y)
        plt.xlabel('times')
        plt.ylabel('values')
        plt.show()
        
if __name__ =="__main__":
        draw_scatter()
        
        
