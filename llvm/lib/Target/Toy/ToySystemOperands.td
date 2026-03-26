class SysReg<string name, bits<12> encoding> {
  string Name       = name;
  bits<12> Encoding = encoding;
}

def CYCLE    : SysReg<"cycle",    0xC00>;
def TIME     : SysReg<"time",     0xC01>;
def INSTRET  : SysReg<"instret",  0xC02>;

def CYCLEH   : SysReg<"cycleh",   0xC80>;
def TIMEH    : SysReg<"timeh",    0xC81>;
def INSTRETH : SysReg<"instreth", 0xC82>;

def SysRegEncodings : GenericEnum {
  let FilterClass = "SysReg";
  let NameField = "Name";
  let ValueField = "Encoding";
}

def SysRegsList : GenericTable {
  let FilterClass = "SysReg";
  let Fields = [ "Name", "Encoding" ];
  let PrimaryKey = [ "Encoding" ];
  let PrimaryKeyName = "lookupSysRegByEncoding";
}

def lookupSysRegByName : SearchIndex {
  let Table = SysRegsList;
  let Key = [ "Name" ];
}
