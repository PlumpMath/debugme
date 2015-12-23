import func Debugme.debugme_install

@inline(never)
func foo() {
    let v = [1, 2, 3]
    print(v[4])
}

@inline(never)
func bar() {
    foo()
}

debugme_install()
print("Hello")
bar()
