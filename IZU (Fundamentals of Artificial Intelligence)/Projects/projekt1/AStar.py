

import math


START = [6, 7]
FINISH = [2, 4]

# COLUMN x ROW
FIELD = [[8,8,9,9,9,9,3,9,6,7], 
         [8,8,9,'Z','Z','Z',3,9,6,7], 
         [8,6,9,'Z',2,5,3,8,5,7], 
         [8,7,9,'Z',6,4,3,8,7,5], 
         ['Z','Z','Z','Z','Z','Z',3,'Z','Z','Z'], 
         [9,9,9,'Z',8,3,9,9,'Z',8], 
         [7,9,9,'Z',9,3,4,2,'Z',7], 
         [7,9,9,'Z',9,3,7,8,'Z',8], 
         [8,9,9,9,9,3,9,8,7,7], 
         [9,9,9,7,6,3,7,9,8,9]]


class Node:
    
    def __init__(self, pos:list, parent=None) -> None:
        self.pos = pos
        self.parent = parent
        self.g = 0 if parent == None else parent.g + self.__getFIELDval(pos)
        self.h = round(math.sqrt((pos[0] - FINISH[0])**2 + (pos[1] - FINISH[1])**2), 2)
        self.f = self.g + self.h


    def addParent(self, node):
        self.parent = node


    def __getFIELDval(self, pos:list) -> int:
        return FIELD[pos[0]][pos[1]]
    

    def toStr(self) -> str:
        return (str(self.pos[0]) + str(self.pos[1]))


class Field:

    def __init__(self, start, finish) -> None:
        self.start = Node(start)
        self.finish = Node(finish)
        self.open:list[Node] = {}
        self.closed:list[Node] = {}
        # Add start node to opened
        self.open[self.start.toStr()] = self.start
        # Help table saves to print
        self.helpTable = {}


    def isFinish(self, node:Node) -> bool:
        return node.pos != self.finish.pos
    

    def __neighbours(self, node:Node) -> list[Node]:
        n = []
        for i in range(-1, 2):
            for j in range(-1, 2):
                if not(i == 0 and j == 0) and (0 <= (node.pos[0] + i) <= 9) and (0 <= (node.pos[1] + j) <= 9):
                    if FIELD[node.pos[0] + i][node.pos[1] + j] != 'Z':
                        n.append(Node([node.pos[0] + i, node.pos[1] + j], node))
        return n


    def __checkProccessed(self, node:Node) -> bool:
        if node.toStr() in self.open:
            if node.f < self.open[node.toStr()].f:
                return True
            else:
                return False
        if node.toStr() in self.closed:
            return False
        return True


    def expand(self, node:Node) -> list[Node]:
        del self.open[node.toStr()]
        self.closed[node.toStr()] = node
        for n in self.__neighbours(node):
            if self.__checkProccessed(n):
                self.open[n.toStr()] = n
                # help table form assigment
                if n.toStr() in self.helpTable:
                    self.helpTable[n.toStr()] = (self.helpTable[n.toStr()] + "  =>  " + f"{n.pos}   {n.g}   {n.h : .2f}   {n.f : .2f}")
                else:
                    self.helpTable[n.toStr()] = f"{n.pos}   {n.g}   {n.h : .2f}   {n.f : .2f}"

        return self.open

    
    def minNode(self) -> Node:
        if self.finish.toStr() in self.open:
            self.closed[self.finish.toStr()] = self.open[self.finish.toStr()]
            del self.open[self.finish.toStr()]
            return self.closed[self.finish.toStr()]
        minVal = list(self.open.values())[0]
        for v in self.open.values():
            if v.f < minVal.f:
                minVal = v
        return minVal
        

    def printPath(self, lastNode) -> None:        
        iter:Node = lastNode
        toPrint:list = []
        print("Final path: ")
        while iter != None:
            toPrint.insert(0, iter)
            iter = iter.parent
        for i in toPrint:
            print(i.pos, end=', ')
        print('\n', end='')


    def printHelpTable(self) -> None:
        print("Help Table Values: ")
        print("  pos    g      h        f")
        for val in self.helpTable.values():
            print(val)


if __name__ == "__main__":
    
    field = Field(START, FINISH)

    curr_node = field.start

    iter = 1


    while field.isFinish(curr_node):
        # Take curr_node and check neighbours
        a = field.expand(curr_node)
        a = a.values()
        # Just print
        ###
        print(f"#{iter}. => {curr_node.pos}: {curr_node.f : .2f}")
        print(f"Open: ")
        line = 1
        for val in a:
            print(f"{line : <4}: {val.pos},{val.f : .2f}, {val.parent.pos}")
            line += 1
        print(f"Closed: ")
        line = 1
        for val in field.closed.values():
            print(f"{line : <4}: {val.pos},{val.f : .2f},{val.parent.pos if val.parent != None else 'NULL'}")
            line += 1
        print('------------------------')
        iter += 1
        ###

        # take next node with minumum f
        curr_node = field.minNode()

    field.printPath(curr_node)
    field.printHelpTable()