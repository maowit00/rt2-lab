
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
        Vorbereitungsaufgabe 10,
        Versuch Motorboard - Teil 1
      ]
    ),
    line(length: 100%, stroke: 0.5pt)
  )
)

// - -- - -- - -- - -- - -- - -- - --

#let dual-figure-page(fig1, fig2) = layout(size => {

  let cell-height = (size.height - 1cm) / 2

  grid(
    columns: (1fr),
    rows: (cell-height, cell-height),
    gutter: 1cm,

    align(center + horizon)[
      #fig1
    ],

    align(center + horizon)[
      #fig2
    ],
  )
})

// - -- - -- - -- - -- - -- - -- - --

#dual-figure-page(
  figure(
    image("vb10_n.svg"),
    caption: [*Drehzahl* Sollwertsprung von 3000 auf 3100 U/min]
  ),
  figure(
    image("vb10_i.svg"),
    caption: [*Motorstrom* Sollwertsprung von 3000 auf 3100 U/min]
  )
)