int main() {
    asm volatile ("push (%eax)");
    return 0;
}