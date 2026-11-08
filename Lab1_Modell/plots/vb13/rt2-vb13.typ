
#set text(lang: "de", size: 11pt)

#set page(paper: "a4")

#set document(
  title: [RT2 Lab1],
  author: "o-manoli",
  date: datetime.today()
)

#set page(
  header: stack(
    dir: ttb,
    spacing: 0.5em,
    grid(
      columns: (1fr, 2fr),
      align(left)[Labor Regelungstechnik 2],
      align(right)[
        Vorbereitungsaufgabe 13,
        Versuch Motorboard - Teil 1
      ]
    ),
    line(length: 100%, stroke: 0.5pt)
  )
)



// - -- - -- - -- - -- - -- - -- - --

= RT2 Lab1

== Vorbereitungsaufgabe 13.2 Simulation

#figure(
  image("vb13_n.svg"),
  caption: [*Drehzahl* Sollwertsprung von 0 auf 10U]
)

#figure(
  image("vb13_i.svg"),
  caption: [*Motorstrom* Sollwertsprung von 0 auf 10U]
)

#figure(
  image("vb13_phi.svg"),
  caption: [*Position* Sollwertsprung von 0 auf 10U]
)

