local AAdventureCharacter = {}

function AAdventureCharacter:beginplay()
    self.Super:beginplay()
    print("AdventureCharacter BeginPlay in Lua!")
end

return Class(nil, nil, AAdventureCharacter)

