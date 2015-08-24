class Item(Object):

    def __init__(self,string):
        self.key=string

    def hash():
        pass

class HashTable(Object):

    def __init__(self,size):
        self.data=[ [] ]#lista de listas
        self.size=size
        self.cs=0#current size

    def get(self,key):
        return self.data[key]

    def insert(self,item):
        if self.cs < self.size :
            index = item.hash()
            self.data[index].append(item)
            self.cs+=1

    def get(self,item):
        hash = item.hash(key)#index
        for i,it in enumerate(self.data[hash]):
            if it.key == key:
                return self.hashTable[hash]
        print (" Not in table")            
        return None

