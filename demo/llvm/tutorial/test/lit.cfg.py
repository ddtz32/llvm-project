import lit.formats
from lit.llvm import llvm_config

config.name = "llvm-tutorial"
config.test_format = lit.formats.ShTest(execute_external=False)
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(config.custom_build_dir, "test")
config.suffixes = [".ll"]

# llvm settings, use FileCheck in lit RUN command
llvm_config.use_default_substitutions()
llvm_config.add_tool_substitutions(["opt"])

# 映射 %lib
config.substitutions.append(("%lib", config.custom_lib_dir))
config.substitutions.append(("%ext", config.shared_lib_ext))
