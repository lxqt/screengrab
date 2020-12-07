use strict;

binmode(STDOUT, ":encoding(utf8)");
binmode(STDERR, ":encoding(utf8)");

my $desktop_in = $ARGV[0];
my $filename_base = $ARGV[1];
my @translation_files = glob($ARGV[2]);

my $section_re = qr/^\[([^\]]+)]/o;
my $lang_re = qr/^.*${filename_base}_([^.]+)\..+$/o;
my $strip_re = qr/#TRANSLATIONS_DIR=/o;

sub flush_translations {
    my ($curr_section) = @_;
    if (defined $curr_section) {
        my $transl_yaml_re = qr/^${curr_section}\/([^: ]+) ?: *([^ ].*)$/;
        foreach my $file (@translation_files) {
            my $language = ($file =~ $lang_re ? "[$1]" : '');
            open(my $trans_fh, '<:encoding(UTF-8)', $file) or next;
            while (my $trans_l = <$trans_fh>) {
                if ($trans_l =~ $transl_yaml_re)
                {
                    my ($key, $value) = ($1, $2);
                    $value =~ s/^\s+|\s+$//; $value =~ s/^['"]//; $value =~ s/['"]$//;
                    if (length($value))
                    {
                        # Don't flush empty (untranslated) strings
                        print(STDOUT "$key$language=$value\n");
                    }
                }
            }
            close($trans_fh);
        }
    }
}


open(my $fh, '<:encoding(UTF-8)', $desktop_in) or die "Could not open file '$desktop_in' $!";
my $curr_section = undef;
while (my $line = <$fh>) {
    if ($line =~ $section_re) {
        flush_translations($curr_section);
        $curr_section = $1;
    }
    $line =~ $strip_re or print(STDOUT $line);
}
flush_translations($curr_section);

close($fh);
