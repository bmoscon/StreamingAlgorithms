class Bucket(object):
    def __init__(self, v):
        self.val = v
        self.items = []

    def insert(self, e):
        self.items.append(e)
    
    def min(self):
        return self.items[0]

    def size(self):
        return len(self.items)

    def remove(self, e):
        self.items.remove(e)

    def value(self):
        return self.val

    def __lt__(self, other):
        return self.val < other.value()

    def __str__(self):
        s = "Bucket " + str(self.val) + "\n"
        s += str(self.items)
        return s
        


class StreamSummary(object):
    def __init__(self, size):
        self.size = size
        # maps items to buckets
        self.bucket_map = {}
        # sorted list of buckets (in order by value)
        self.bucket_list = []

        self.bucket_list.append(Bucket(1))

        
    def __str__(self):
        s = ""
        for b in self.bucket_list:
            s += str(b) 
            s += "\n"
        return s

    def add(self, e):
        if e in self.bucket_map:
            b = self.bucket_map[e]
            v = b.value()
            
            b.remove(e)
            if b.size() == 0:
                self.bucket_list.remove(b)
            
            for i in self.bucket_list:
                if i.value() == v + 1:
                    i.insert(e)
                    self.bucket_map[e] = i
                    return
            
            b = Bucket(v + 1)
            b.insert(e)
            self.bucket_map[e] = b
            self.bucket_list.append(b)
            self.bucket_list.sort()
        else:
            if len(self.bucket_map.keys()) < self.size:
                if self.bucket_list[0].value() > 1:
                    self.bucket_list.insert(0, Bucket(1))
                    self.bucket_list[0].insert(e)
                    self.bucket_map[e] = self.bucket_list[0]
                else:
                    self.bucket_map[e] = self.bucket_list[0]
                    self.bucket_list[0].insert(e)
            else:
                ejected = self.bucket_list[0].min()
                ejected_val = self.bucket_list[0].value()
                del self.bucket_map[ejected]
                self.bucket_list[0].remove(ejected)
                if self.bucket_list[0].size() == 0:
                    del self.bucket_list[0]

                for i in self.bucket_list:
                    if i.value() == ejected_val + 1:
                        i.insert(e)
                        self.bucket_map[e] = i
                        return

                b = Bucket(ejected_val + 1)
                b.insert(e)
                self.bucket_list.append(b)
                self.bucket_list.sort()
                self.bucket_map[e] = b

