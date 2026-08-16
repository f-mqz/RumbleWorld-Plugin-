#include <3ds.h>
#include <CTRPluginFramework.hpp>

using namespace CTRPluginFramework;

// Adresses globales
u32 g_diamondsAddr = 0;
u32 g_coinsAddr = 0;
u32 g_rankAddr = 0;
u32 g_healthAddr = 0;
u32 g_megaAddr = 0;
u32 g_mineAddr = 0;
u32 g_captureAddr = 0;
u32 g_playerPokemonAddr = 0;

bool g_infiniteHealth = false;
bool g_autoCapture = false;
bool g_infiniteMega = false;
bool g_infiniteMine = false;
bool g_oneHitKO = false;

// AOB Scan
u32 AOBScan(const u8 *pattern, const char *mask, u32 startAddr, u32 size) {
    for (u32 i = 0; i < size; i++) {
        bool found = true;
        for (u32 j = 0; mask[j]; j++) {
            if (mask[j] == 'x' && ((u8*)startAddr)[i + j] != pattern[j]) {
                found = false; break;
            }
        }
        if (found) return startAddr + i;
    }
    return 0;
}

u32 AOBScanModule(const u8 *pattern, const char *mask) {
    u32 result = AOBScan(pattern, mask, 0x10000000, 0x01000000);
    if (result) return result;
    return AOBScan(pattern, mask, 0x30000000, 0x00800000);
}

bool WriteU32(u32 addr, u32 value) {
    if (addr < 0x10000000 || addr > 0x40000000) return false;
    *(u32*)addr = value; return true;
}
bool WriteU16(u32 addr, u16 value) {
    if (addr < 0x10000000 || addr > 0x40000000) return false;
    *(u16*)addr = value; return true;
}
bool WriteU8(u32 addr, u8 value) {
    if (addr < 0x10000000 || addr > 0x40000000) return false;
    *(u8*)addr = value; return true;
}
u32 ReadU32(u32 addr) {
    if (addr < 0x10000000 || addr > 0x40000000) return 0;
    return *(u32*)addr;
}

// Pokemon data
struct PkmEntry { u16 id; const char* name; };
const PkmEntry PKM_LIST[] = {
    {1,"Bulbizarre"},{4,"Salameche"},{7,"Carapuce"},{25,"Pikachu"},
    {133,"Evoli"},{448,"Lucario"},{716,"Xerneas"},{717,"Yveltal"},
    {718,"Zygarde"},{719,"Diancie"},{720,"Hoopa"},{721,"Volcanion"}
};

void InitAddresses() {
    OSD::Notify("Scan AOB...");
    u8 dpat[] = {0x32,0x00,0x00,0x00};
    g_diamondsAddr = AOBScanModule(dpat, "xxxx");
    if (g_diamondsAddr) OSD::Notify("Diamants OK");
    OSD::Notify("Scan fini");
}

void MenuMoney() {
    Keyboard kb("1=Diamants 9999\n2=Pieces 999999\n3=Mine Inf");
    u32 c = 1; if (kb.Open(c) != 0) return;
    if (c==1 && g_diamondsAddr) { WriteU32(g_diamondsAddr,9999); OSD::Notify("Diamants=9999"); }
    if (c==2 && g_coinsAddr) { WriteU32(g_coinsAddr,999999); OSD::Notify("Pieces=999999"); }
    if (c==3) { g_infiniteMine=!g_infiniteMine; OSD::Notify(g_infiniteMine?"Mine:ON":"Mine:OFF"); }
}

void MenuPokemon() {
    Keyboard kb("1=Changer Pokemon\n2=Capture Auto\n3=Scan Pokemon");
    u32 c = 1; if (kb.Open(c) != 0) return;
    if (c==1) {
        Keyboard kb2("ID Pokemon (1-721)"); u32 id = 25;
        if (kb2.Open(id)==0 && g_playerPokemonAddr) {
            WriteU16(g_playerPokemonAddr, (u16)id);
            Keyboard kb3("Puissance"); u32 p = 10000; kb3.Open(p);
            WriteU32(g_playerPokemonAddr + 4, p);
            OSD::Notify("Pokemon change !");
        }
    }
    if (c==2) { g_autoCapture=!g_autoCapture; OSD::Notify(g_autoCapture?"Cap:ON":"Cap:OFF"); }
    if (c==3) {
        Keyboard kb2("ID actuel"); u32 id = 25;
        if (kb2.Open(id)==0) {
            u8 pat[] = {(u8)(id&0xFF), (u8)((id>>8)&0xFF)};
            g_playerPokemonAddr = AOBScanModule(pat, "xx");
            OSD::Notify(g_playerPokemonAddr?"Trouve !":"Non trouve");
        }
    }
}

void MenuCombat() {
    Keyboard kb("1=Vie Inf\n2=Mega Inf\n3=Invincible\n4=One-Hit KO");
    u32 c = 1; if (kb.Open(c) != 0) return;
    if (c==1) { g_infiniteHealth=!g_infiniteHealth; OSD::Notify(g_infiniteHealth?"Vie:ON":"Vie:OFF"); }
    if (c==2) { g_infiniteMega=!g_infiniteMega; OSD::Notify(g_infiniteMega?"Mega:ON":"Mega:OFF"); }
    if (c==3) { g_infiniteHealth=true; OSD::Notify("Invincible!"); }
    if (c==4) { g_oneHitKO=!g_oneHitKO; OSD::Notify(g_oneHitKO?"OHKO:ON":"OHKO:OFF"); }
}

void MenuOther() {
    Keyboard kb("1=Rang 9999 (3GX)\n2=Hex Editor\n3=Scan Adresses\n4=Infos");
    u32 c = 1; if (kb.Open(c) != 0) return;
    if (c==1 && g_rankAddr) { WriteU32(g_rankAddr,9999); OSD::Notify("Rang 3GX!"); }
    if (c==2) {
        Keyboard kb2("Adresse hex"); std::string s = "10000000"; kb2.Open(s);
        u32 addr = 0; try { addr = std::stoul(s, nullptr, 16); } catch(...) {}
        if (addr) {
            std::string res = "";
            for (u32 i = 0; i < 256; i += 16) {
                char line[64]; snprintf(line, sizeof(line), "%08X: ", addr + i); res += line;
                for (u32 j = 0; j < 16; j++) {
                    u8 v = (addr + i + j < 0x40000000) ? *(u8*)(addr + i + j) : 0;
                    snprintf(line, sizeof(line), "%02X ", v); res += line;
                }
                res += "\n";
            }
            MessageBox("Hex", res)();
        }
    }
    if (c==3) InitAddresses();
    if (c==4) {
        std::string info = "Adresses:\n";
        info += "Diamants: " + (g_diamondsAddr ? Hex(g_diamondsAddr) : "N/A") + "\n";
        info += "Pieces: " + (g_coinsAddr ? Hex(g_coinsAddr) : "N/A") + "\n";
        info += "Rang: " + (g_rankAddr ? Hex(g_rankAddr) : "N/A") + "\n";
        info += "Vie: " + (g_healthAddr ? Hex(g_healthAddr) : "N/A") + "\n";
        info += "Pokemon: " + (g_playerPokemonAddr ? Hex(g_playerPokemonAddr) : "N/A");
        MessageBox("Infos", info)();
    }
}

int main() {
    PluginMenu menu("Pokemon Rumble World - Plugin 3GX", 1, 0, 0);
    menu.SynchronizeWithFrame(true);

    MenuFolder *fMoney = new MenuFolder("Monnaie");
    fMoney->Append(new MenuEntry("Menu Monnaie", nullptr, [](MenuEntry*){ MenuMoney(); }));
    fMoney->Append(new MenuEntry("Diamants=9999", nullptr, [](MenuEntry*){ if(g_diamondsAddr)WriteU32(g_diamondsAddr,9999); }));
    fMoney->Append(new MenuEntry("Pieces=999999", nullptr, [](MenuEntry*){ if(g_coinsAddr)WriteU32(g_coinsAddr,999999); }));
    fMoney->Append(new MenuEntry("Mine Inf", nullptr, [](MenuEntry*){ g_infiniteMine=!g_infiniteMine; OSD::Notify(g_infiniteMine?"Mine:ON":"Mine:OFF"); }));

    MenuFolder *fPkm = new MenuFolder("Pokemon");
    fPkm->Append(new MenuEntry("Changer Pokemon", nullptr, [](MenuEntry*){ MenuPokemon(); }));
    fPkm->Append(new MenuEntry("Capture Auto", nullptr, [](MenuEntry*){ g_autoCapture=!g_autoCapture; OSD::Notify(g_autoCapture?"Cap:ON":"Cap:OFF"); }));

    MenuFolder *fCombat = new MenuFolder("Combat");
    fCombat->Append(new MenuEntry("Vie Inf", nullptr, [](MenuEntry*){ g_infiniteHealth=!g_infiniteHealth; OSD::Notify(g_infiniteHealth?"Vie:ON":"Vie:OFF"); }));
    fCombat->Append(new MenuEntry("Mega Inf", nullptr, [](MenuEntry*){ g_infiniteMega=!g_infiniteMega; OSD::Notify(g_infiniteMega?"Mega:ON":"Mega:OFF"); }));
    fCombat->Append(new MenuEntry("Invincible", nullptr, [](MenuEntry*){ g_infiniteHealth=true; OSD::Notify("Invincible!"); }));
    fCombat->Append(new MenuEntry("One-Hit KO", nullptr, [](MenuEntry*){ g_oneHitKO=!g_oneHitKO; OSD::Notify(g_oneHitKO?"OHKO:ON":"OHKO:OFF"); }));

    MenuFolder *fOther = new MenuFolder("Autre");
    fOther->Append(new MenuEntry("Rang 3GX", nullptr, [](MenuEntry*){ if(g_rankAddr)WriteU32(g_rankAddr,9999); }));
    fOther->Append(new MenuEntry("Hex Editor", nullptr, [](MenuEntry*){ MenuOther(); }));
    fOther->Append(new MenuEntry("Scan AOB", nullptr, [](MenuEntry*){ InitAddresses(); }));

    menu += new MenuEntry("[AUTO] Vie", [](MenuEntry*){ if(g_infiniteHealth&&g_healthAddr)WriteU32(g_healthAddr,9999); });
    menu += new MenuEntry("[AUTO] Mega", [](MenuEntry*){ if(g_infiniteMega&&g_megaAddr)WriteU32(g_megaAddr,9999); });
    menu += new MenuEntry("[AUTO] Mine", [](MenuEntry*){ if(g_infiniteMine&&g_mineAddr)WriteU32(g_mineAddr,0); });
    menu += new MenuEntry("[AUTO] Capture", [](MenuEntry*){ if(g_autoCapture&&g_captureAddr)WriteU8(g_captureAddr,1); });

    menu += fMoney; menu += fPkm; menu += fCombat; menu += fOther;

    InitAddresses();
    menu.Run();
    return 0;
}
