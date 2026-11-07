
#set text(lang: "de", size: 11pt)

#set page(paper: "a4")

#show heading: set block(below: 1.3em)

#set document(
  title: [RT2 Lab1],
  author: "o-manoli",
  date: datetime.today()
)

#show heading: set block(below: 1.3em)

#show heading.where(level: 1): set text(size: 1.8em)
#show heading.where(level: 2): set text(size: 1.4em)

#set page(
  header: stack(
    dir: ttb,
    spacing: 0.5em,
    grid(
      columns: (1fr, 2fr),
      align(left)[Labor Regelungstechnik 2],
      align(right)[Versuch Motorboard – Teil 1]
    ),
    line(length: 100%, stroke: 0.5pt)
  ),
  footer: context [
    #line(length: 100%, stroke: 0.5pt)
    #align(
      center,
      counter(page).display(
        "1 / 1",
        both: true,
      )
    )
  ]
)

// - -- - -- - -- - -- - -- - -- - --

#let dual-figure-page(fig1, fig2) = layout(size => {

  let cell-height = (size.height - 2.7cm) / 2

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


= RT2 Lab1 WS26-27

// - -- - -- - --

== Vorbereitungsaufgabe 7: Simulationen der Strecke

#figure(
  image(
    "Strecke.svg",
    width: 100%
  ),
  caption: [Strecke.slx]
)

#v(2cm)

#line(length: 100%)

#v(1cm)

// - -- - -- - --

#outline(
  title: [List of Figures],
  target: figure.where(kind: image),
)

// - -- - -- - -- - -- - -- - -- - --

#pagebreak()

== Aufgabenteil a)

#dual-figure-page(
  figure(
    image("vb7a_n.svg"),
    caption: [*Drehzahl* Sprung von $u_("Motor,in")$ $0 "V"$ auf $4 "V"$, kein Strom $I_("G,DC")$]
  ),
  figure(
    image("vb7a_i.svg"),
    caption: [*Motorstrom* Sprung von $u_("Motor,in")$ $0 "V"$ auf $4 "V"$, kein Strom $I_("G,DC")$]
  )
)

// - -- - -- - -- - -- - -- - -- - --

#pagebreak()

== Aufgabenteil b)

#dual-figure-page(
  figure(
    image("vb7b_n.svg"),
    caption: [*Drehzahl* $u_("Motor,in")$ von $0 "V"$ auf $4 "V"$, Generator an Lampe]
  ),
  figure(
    image("vb7b_i.svg"),
    caption: [*Motorstrom* $u_("Motor,in")$ von $0 "V"$ auf $4 "V"$, Generator an Lampe]
  )
)

// - -- - -- - -- - -- - -- - -- - --

#pagebreak()

== Aufgabenteil c)

#dual-figure-page(
  figure(
    image("vb7c_n.svg"),
    caption: [*Drehzahl* $u_("Motor,in")$ $0 "V"$ auf $4 "V"$, Generator-Stromquelle, $0$ auf $I_("G,DC") = +200 "mA"$]
  ),
  figure(
    image("vb7c_i.svg"),
    caption: [*Motorstrom* $u_("Motor,in")$ $0 "V"$ auf $4 "V"$, Generator-Stromquelle, $0$ auf $I_("G,DC") = +200 "mA"$]
  )
)

// - -- - -- - -- - -- - -- - -- - --

#pagebreak()

== Aufgabenteil d)

#dual-figure-page(
  figure(
    image("vb7d_n.svg"),
    caption: [*Drehzahl* $u_("Motor,in")$ $0 "V"$ auf $4 "V"$, Generator-Stromquelle, $0$ auf $I_("G,DC") = -200 "mA"$]
  ),
  figure(
    image("vb7d_i.svg"),
    caption: [*Motorstrom* $u_("Motor,in")$ $0 "V"$ auf $4 "V"$, Generator-Stromquelle, $0$ auf $I_("G,DC") = -200 "mA"$]
  )
)

// - -- - -- - -- - -- - -- - -- - --

