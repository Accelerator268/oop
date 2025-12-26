#pragma once
#include <memory>
#include <fstream>

class NPC;

class IFightObserver {
public:
    virtual ~IFightObserver() = default;
    virtual void onFight(const std::shared_ptr<NPC>& attacker,
                        const std::shared_ptr<NPC>& defender,
                        bool win) = 0;
};

class ConsoleObserver : public IFightObserver {
public:
    void onFight(const std::shared_ptr<NPC>& attacker,
                const std::shared_ptr<NPC>& defender,
                bool win) override;
};

class FileObserver : public IFightObserver {
private:
    std::ofstream logFile;
    
public:
    FileObserver(const std::string& filename = "log.txt");
    ~FileObserver();
    
    void onFight(const std::shared_ptr<NPC>& attacker,
                const std::shared_ptr<NPC>& defender,
                bool win) override;
};