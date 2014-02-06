require "hs_base"

-- 双向链表节点结构
-- slist_node = { next, prev, value }

slist = { head=nil, tail=nil, length=0 }

function slist:new()
    return base_object(nil, self) 
end

function slist:push_back(new_value)
    local new_node = {}
    new_node.value = new_value
    new_node.next = nil
    if self.length == 0 then
        new_node.prev = nil
        self.head = new_node
    else
        new_node.prev = self.tail
        self.tail.next = new_node
    end
    self.tail = new_node
    self.length = self.length + 1
end

function slist:pop_back()
    if self.length == 0 then
        return nil
    end
    local pop_node = self.head
    local back_value = self.tail.value
    if pop_node.next ~= nil then
        pop_node.next.prev = nil
    end
    self.head = pop_node.next
    pop_node = nil
    self.length = self.length - 1
    return back_value
end

function slist:push_front(new_value)
    local new_node = {}
    new_node.value = new_value
    new_node.prev = nil
    if self.length == 0 then
        self.tail = new_node
        new_node.next = nil
    else
        local cur_head = self.head
        cur_head.prev = new_node
        new_node.next = cur_head
    end
    self.head = new_node
    self.length = self.length + 1
end

function slist:pop_front()
    if self.length == 0 then
        return nil
    end
    local front_node = self.head
    local front_value = self.head.value
    self.head = self.head.next
    if self.head ~= nil then
        self.head.prev = nil
    end
    self.length = self.length - 1
    return front_value
end

-- 插入index位置，index从0开始
function slist:insert(index, new_value)
    local count = 1
    local node = self.head
    while true do
        if node == nil or count >= index then
            break
        end
        node = node.next
        count = count + 1
    end
    if node == nil or count < index then
        return false
    end
    local new_node = {}
    new_node.value = new_value
    new_node.next = node
    new_node.prev = node.prev
    if node.prev ~= nil then
        node.prev.next = new_node
    end
    node.prev = new_node
    self.length = self.length + 1
    return true
end

function slist:delete(index)
    local count = 1
    local node = self.head
    while true do
        if node == nil or count >= index then
            break
        end
        node = node.next
        count = count + 1
    end
    if node == nil or count < index then
        return nil
    end
    local del_value = node.value
    if node.prev ~= nil then
        node.prev.next = node.next
    end
    if node.next ~= nil then
        node.next.prev = node.prev
    end
    node = nil
    self.length = self.length - 1
    return del_value 
end