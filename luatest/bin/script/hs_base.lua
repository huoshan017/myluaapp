function base_object(object, meta)
    object = object or {}
    setmetatable(object, meta)
    meta.__index = meta
    return object
end