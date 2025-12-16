# Data Communication - Socket Programming Projesi

Bu proje, Veri Haberleşmesi dersi kapsamında, C++ dili ve Socket API kullanılarak geliştirilmiş bir istemci-sunucu (Client-Server) uygulamasıdır.

Projenin Amacı
Proje, ağ üzerinden veri iletimini simüle etmek ve bu iletim sırasında kullanılan hata tespit (error detection) algoritmalarını pratik olarak uygulamayı amaçlar.

Uygulanan Algoritmalar
Kod içerisinde veri bütünlüğünü sağlamak için aşağıdaki yöntemler kullanılmıştır:
1.  Parity Check (Eşlik Denetimi): Veriye tek boyutlu (Even/Odd) bit ekleyerek basit hata tespiti.
2.  2D Parity (Matrix Parity / LRC): Veriyi matris şeklinde işleyerek hem satır hem sütun bazlı hata kontrolü.
3   CRC (Cyclic Redundancy Check): Polinom bölme işlemi (MOD-2 aritmetiği) ile güçlü hata tespiti.
4.  Hamming Code: Verideki hataları hem tespit eden hem de (tek bitlik) düzeltebilen algoritma.
5.  Checksum (Internet Checksum): Veri bloklarının toplamını alarak (1's complement) bütünlük doğrulama.

Kurulum ve Çalıştırma Sırası

Projenin hatasız çalışması için dosyaların mutlaka aşağıdaki sıra ile çalıştırılması gerekmektedir:

1.  Adım 1 (İstemci 2): İlk olarak `client2.exe` dosyasını çalıştırın.
2.  Adım 2 (Sunucu): Ardından `server.exe` dosyasını çalıştırın.
3.  Adım 3 (İstemci 1): Son olarak `client1.exe` dosyasını çalıştırın.

**Önemli Not:** Sıralama değiştirilirse bağlantı veya senkronizasyon hataları oluşabilir.
