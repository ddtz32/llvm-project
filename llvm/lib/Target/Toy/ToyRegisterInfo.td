class ToyReg<string name>: Register<name> {
  let Namespace = "Toy";
}

foreach Index = 0-32 in
  def X # Index : ToyReg<"X" # Index>;

def GPR : RegisterClass<"Toy", [i32], 32, (add (sequence "X%u", 0, 31))>;
