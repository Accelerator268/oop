#pragma once
#include <memory>

class NPC;
class Dragon;
class Knight;
class Pegasus;

class IFightVisitor {
public:
    virtual ~IFightVisitor() = default;
    
    virtual bool visit(const std::shared_ptr<Dragon>& defender,
                      const std::shared_ptr<NPC>& attacker) = 0;
    
    virtual bool visit(const std::shared_ptr<Knight>& defender,
                      const std::shared_ptr<NPC>& attacker) = 0;
    
    virtual bool visit(const std::shared_ptr<Pegasus>& defender,
                      const std::shared_ptr<NPC>& attacker) = 0;
};

class FightVisitor : public IFightVisitor {
public:
    bool visit(const std::shared_ptr<Dragon>& defender,
               const std::shared_ptr<NPC>& attacker) override;
    
    bool visit(const std::shared_ptr<Knight>& defender,
               const std::shared_ptr<NPC>& attacker) override;
    
    bool visit(const std::shared_ptr<Pegasus>& defender,
               const std::shared_ptr<NPC>& attacker) override;
};