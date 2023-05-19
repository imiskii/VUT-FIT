
class Point():
    def __init__(self, x = 0, y = 0) -> None:
        self.x = x
        self.y = y

    def __sub__(self,sub_point):
        return self.x - sub_point.x, self.y - sub_point.y

p0 = Point()
print(p0) # should be: Point(0,0)
p1 = Point(3, 4)
result = p0-p1
print(result) # should be: Point(-3,-4)
